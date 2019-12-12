#load "Init.fsx"

open System
open System.Threading
open MathNet.Numerics.Distributions
open FSharp.Collections.ParallelSeq
open OpenAir.LDPC
open OpenAir.Bench

let encoder = OpenAir.LDPC.SimpleEncoder()
let decoder = new ThreadLocal<OpenAir.LDPC.Decoder>(fun _ -> new OpenAir.LDPC.Decoder())

let input = Sources.Random.Make (Configuration.MAX_BLOCK_LENGTH, 13)
let configuration = Configuration.MkFromBlockLength(input.Length, 2, 3)

let channel_input = encoder.EncodeFull(input, configuration)
let temp_channel_output = new ThreadLocal<float []>(fun _ -> Array.init channel_input.Length (fun _ -> 0.0))
let temp_decoder_input = new ThreadLocal<sbyte []>(fun _ -> Array.init channel_input.Length (fun _ -> 0y))

[<StructuredFormatDisplay("{Display}")>]
type Result = {
    Seed        : int
    SNR         : float
    ExtraBits   : int
    Iterations  : int
    IsCorrect   : bool
    CodeRatio   : float
    UncodedBER  : float
    BitErrors   : int
}
with
    member this.Display =
        let code_ratio = 100.0 * (this.CodeRatio - 1.0)
        let uncoded_ber = 100.0 * this.UncodedBER
        Printf.sprintf "%6b %5.2f %5.2f%% %8.5f%% %d"
            this.IsCorrect this.SNR code_ratio uncoded_ber this.BitErrors
    override this.ToString() = this.Display

let inline mapi_in_place<'T> (f : int -> 'T -> 'T) (input : 'T []) : 'T[] =
    for i in 0 .. (input.Length - 1) do
        input.[i] <- f i input.[i]
    input

let evaluate (input : byte[]) seed (snr : SNR) extra iterations =
    let rng = Random(seed)

    let start = channel_input.Start
    let stop = start + input.Length * 8 + extra
    let ratio = (float(stop) - float(start)) / (8.0 * float(input.Length))

    let sigma = 1.0 / Math.Sqrt(2.0 * snr.linear)
    let denom = sigma / (4.0 * 4.0)

    let channel_output =
        channel_input.Buffer
        |> Quantizer.Binary.Array.map_byte_in_place temp_channel_output.Value
        |> Channel.Gaussian.Array.add_in_place (rng, snr)
        |> mapi_in_place (
            fun  i v -> if i < start || i > stop then 0.0 else v / denom
        )
        |> Quantizer.Array.quantize_in_place 8 temp_decoder_input.Value

    let hard_decision_errors =
        Seq.fold2 (
            fun errors ideal channel ->
                let error =
                    if channel > 0y && ideal = 1uy   then 1
                    elif channel < 0y && ideal = 0uy then 1
                    else 0
                errors + error
        ) 0 channel_input.Buffer channel_output

    let uncoded_bit_error_rate = float(hard_decision_errors) / (float(stop - start))

    let result = decoder.Value.Decode(channel_output, configuration, iterations, input.Length)
    let is_correct = Array.forall2 (=) input result

    let bit_errors =
        if is_correct then 0
        else
            Seq.map2 (
                fun (l:byte) (r:byte) ->
                    count_bits (l ^^^ r)
            ) input result
            |> Seq.sum

    {
        Seed        = seed
        SNR         = snr.value
        ExtraBits   = extra
        Iterations  = iterations
        IsCorrect   = is_correct
        CodeRatio   = ratio
        UncodedBER  = uncoded_bit_error_rate
        BitErrors   = bit_errors
    }

let snr = Metrics.snr 5.0
let minimum_overhead =
    let rate = Channel.Gaussian.Binary.Capacity snr
    let size = float(8 * Configuration.MAX_BLOCK_LENGTH)
    let overhead = size * (1.0 - rate) / rate
    overhead |> Math.Ceiling |> int

let extra_overhead = 64

let overhead = minimum_overhead + extra_overhead
let code_rate = float(8 * Configuration.MAX_BLOCK_LENGTH) / float(8 * Configuration.MAX_BLOCK_LENGTH + overhead)

let ideal_snr = Channel.Gaussian.Binary.SnrFromCodeRate code_rate
snr.value - ideal_snr.value

#time
[ 1 .. 20_000_000]
|> PSeq.map (fun seed -> evaluate input seed snr overhead 200 )
|> PSeq.filter (fun v -> v.IsCorrect = false)
|> PSeq.iter (printfn "%A")
printfn "DONE"

let ideal_levels (snr : float) =
    let snr_linear = Math.Pow(10.0, snr / 10.0)
    Math.Sqrt(1.0 + snr_linear)

let snr_linear = (Metrics.snr 3.0).linear
let sigma = 1.0 / Math.Sqrt(2.0 * snr_linear)
let gaussian = Normal(0.0, 1.0, Random(12))

let out2 =
    Seq.map2 (
        fun value noise ->
            let v = if value = 1uy then -1.0 elif value = 0uy then 1.0 else failwithf "Illegal value %d" value
            let nom = v + sigma * noise
            let denom = sigma * sigma / (4.0 * 4.0)
            nom / denom, sigma * noise, nom, sigma

    ) (Sources.Binary.Alternate(20)) (gaussian.Samples())
    //|> quantize 8
    |> Seq.toList

