#I @"../../.paket/load/net472"
#load "MathNet.Numerics.fsx"
#load "NLog.fsx"

#I @"..\x64\Debug\"
#r "OpenAir.NET.dll"

#load "Errors.fs"
#load "Metrics.fs"
#load "Channels.fs"
#load "Quantizer.fs"
#load "Sources.fs"

open System
open OpenAir.LDPC
open OpenAir.Bench

let encoder = OpenAir.LDPC.SimpleEncoder()
let decoder = new OpenAir.LDPC.Decoder()

let input = Sources.Random.Make (Configuration.MAX_BLOCK_LENGTH, 13)
let configuration = Configuration.MkFromBlockLength(input.Length, 2, 3)

let channel_input = encoder.EncodeFull(input, configuration)

let start = channel_input.Start
let stop_ideal = channel_input.Stop
let stop = channel_input.Start + input.Length * 8 + 284
let ratio = (float(stop) - float(start)) / (8.0 * float(input.Length))

let ideal_channel_output =
    Quantizer.Binary.Map.Mk(8, channel_input.Buffer)
    |> Array.mapi (fun  i v -> if i < start || i > stop then 0 else v)
    |> Channel.Ideal.Apply
    |> Seq.map sbyte
    |> Seq.toArray

let ideal_result = decoder.Decode(ideal_channel_output, configuration, 5, input.Length)

Array.forall2 (=) input ideal_result



open MathNet.Numerics.Distributions
let add_gaussian_noise (rng : System.Random) (snr : float) (input : System.Collections.Generic.IReadOnlyList<byte>) : float seq =
    let snr_linear = Math.Pow(10.0, snr / 10.0)
    let sigma = 1.0 / Math.Sqrt(2.0 * snr_linear)

    let gaussian = Normal(0.0, 1.0, rng)
    let samples = gaussian.Samples()
    Seq.map2 (
        fun value noise ->
            let v = if value = 1uy then -1.0 elif value = 0uy then 1.0 else failwithf "Illegal value %d" value
            let nom = v + sigma * noise
            // let denom = sigma * sigma / (4.0 * 4.0)
            let denom = sigma / (4.0 * 4.0)
            nom / denom

    ) input samples

let quantize (qbits : int) (input : float seq) =
    let bits_to_shift = qbits - 1
    let maxlev = float(1 <<< bits_to_shift)

    input
    |> Seq.map (
        fun value ->
            let level =
                if value <= -maxlev     then -maxlev
                elif value >= maxlev    then maxlev - 1.0
                else value
            sbyte(level)
    )

let rec count_bits (number : byte) =
    if number = 0uy then 0
    else 1 + count_bits (number &&& (number - 1uy))

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

let evaluate (input : byte[]) seed snr extra iterations =
    let rng = Random(seed)

    let start = 0
    let stop = input.Length * 8 + extra
    let ratio = (float(stop) - float(start)) / (8.0 * float(input.Length))

    let channel_output =
        channel_input.Buffer
        |> add_gaussian_noise rng snr
        |> Seq.mapi (fun  i v -> if i < start || i > stop then 0.0 else v)
        |> quantize 8
        |> Seq.toArray

    let hard_decision_errors =
        Seq.map2 (
            fun ideal channel ->
                if channel > 0y && ideal = 1uy   then 1
                elif channel < 0y && ideal = 0uy then 1
                else 0
        ) channel_input.Buffer channel_output
        |> Seq.sum

    let uncoded_bit_error_rate = float(hard_decision_errors) / (float(stop - start))

    let result = decoder.Decode(channel_output, configuration, iterations, input.Length)
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
        SNR         = snr
        ExtraBits   = extra
        Iterations  = iterations
        IsCorrect   = is_correct
        CodeRatio   = ratio
        UncodedBER  = uncoded_bit_error_rate
        BitErrors   = bit_errors
    }

[ 1 .. 40] |> List.map (fun seed -> evaluate input seed 3.0 (284 + 1024) 200) |> List.iter (printfn "%A")

let prob_uncoded_error (snr : float) =
    let snr_linear = Math.Pow(10.0, snr / 10.0)
    let sigma = 1.0 / Math.Sqrt(2.0 * snr_linear)
    let gaussian2 = Normal(0.0, sigma, Random(12))
    1.0 - gaussian2.CumulativeDistribution(1.0)

let binary_channel_capacity (snr : float) =
    let p = prob_uncoded_error snr
    let p1 = 1.0 - p
    let h = -p * Math.Log(p, 2.0) - p1 * Math.Log(p1, 2.0)
    1.0 - h

let shannon_capacity (snr : float) =
    let snr_linear = Math.Pow(10.0, snr / 10.0)
    Math.Log(1.0 + snr_linear, 2.0)

let ideal_levels (snr : float) =
    let snr_linear = Math.Pow(10.0, snr / 10.0)
    Math.Sqrt(1.0 + snr_linear)

prob_uncoded_error 3.0
prob_uncoded_error 4.0
prob_uncoded_error 5.0

binary_channel_capacity 3.0
binary_channel_capacity 4.0
binary_channel_capacity 5.0

shannon_capacity 3.0
ideal_levels 3.0


let yyy = [ 3.0..0.01..6.0 ] |> List.map (fun snr -> prob_uncoded_error snr, (1.0 - binary_channel_capacity snr)) |> List.sortBy fst

let snr = 3.0
let snr_linear = Math.Pow(10.0, snr / 10.0)
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
    |> Seq.toList
    //|> quantize 8
    //|> Seq.toList
