#load "Init.fsx"

open System
open System.IO
open System.Threading
open System.Collections.Generic
open MathNet.Numerics
open FSharp.Collections.ParallelSeq
open OpenAir.LDPC
open OpenAir.Bench
open Deedle
open Deedle.``F# Frame extensions``

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
        Printf.sprintf "%6b %5.2f%% %8.5f%% %d"
            this.IsCorrect code_ratio uncoded_ber this.BitErrors
    override this.ToString() = this.Display

let inline mapi_in_place<'T> (f : int -> 'T -> 'T) (input : 'T []) : 'T[] =
    for i in 0 .. (input.Length - 1) do
        input.[i] <- f i input.[i]
    input

let dump_factor = 1.0
let norm_factor = 0.8

let evaluate (input : byte[]) seed errors extra iterations =
    let rng = Random(seed)

    let start = channel_input.Start
    let stop = start + input.Length * 8 + extra
    let ratio = float(stop - start + 1) / (8.0 * float(input.Length))

    let bits_in_error : HashSet<int> =
        Combinatorics.GeneratePermutation(stop - start, rng)
        |> Array.take errors
        |> Array.map (fun v -> v + start)
        |> HashSet

    // printfn "Errors in %A" bits_in_error


    let channel_output =
        channel_input.Buffer
        |> Array.mapi (
            fun i v ->
                let reverse v = if v = 0uy then 1uy else 0uy
                if bits_in_error.Contains(i) then
                    reverse v
                else
                    v
        )
        |> Quantizer.Binary.Array.map_byte_in_place temp_channel_output.Value
        |> mapi_in_place (
            fun  i v ->
                if i < start || i > stop
                then 0.0
                else
                    let f = if bits_in_error.Contains(i) then dump_factor else 1.0
                    if v > 0.0 then 127.0 * f * norm_factor
                    else -128.0 * f * norm_factor
        )
        |> Quantizer.Array.quantize_in_place 8 temp_decoder_input.Value

    let hard_decision_errors =
        Seq.fold2 (
            fun errors ideal channel ->
                //printfn "Ideal=%A Received=%A" ideal channel
                let error =
                    if channel > 0y && ideal = 1uy   then 1
                    elif channel < 0y && ideal = 0uy then 1
                    else 0
                errors + error
        ) 0 channel_input.Buffer channel_output

    let uncoded_bit_error_rate = float(hard_decision_errors) / (float(stop - start + 1))

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
        ExtraBits   = extra
        Iterations  = iterations
        IsCorrect   = is_correct
        CodeRatio   = ratio
        UncodedBER  = uncoded_bit_error_rate
        BitErrors   = bit_errors
    }

evaluate input 13 20 300 200

let iterations = 200
let rng = Random(13)

let result =
    [20..10..500]
    |> PSeq.collect (
        fun errors ->
            [200..20..8000]
            |> PSeq.map (
                fun overhead ->
                    {|
                        BitErrors   = errors
                        ExtraBits   = overhead
                        Results     = evaluate input (rng.Next()) errors overhead iterations
                    |}
                    
            )
    )
    |> PSeq.toList

let frame =
    result
    |> List.map(
        fun v ->
            {|
                BitErrors       = v.BitErrors
                ExtraBits       = v.ExtraBits
                Success         = v.Results.IsCorrect
                UncodedBer      = v.Results.UncodedBER
                DecodedBitErrors= v.Results.BitErrors
                Iterations      = v.Results.Iterations
                IterationsMax   = iterations
                CodeRatio       = v.Results.CodeRatio
                DumpeningFactor = dump_factor
            |}
    )
    |> Frame.ofRecords

let output_directory = Path.Combine(__SOURCE_DIRECTORY__, "results")
frame.SaveCsv(Path.Combine(output_directory, "inserted_bit_errors_f10_n08.csv"))

