#load "Init.fsx"

open System
open OpenAir.LDPC
open OpenAir.Bench

let encoder = OpenAir.LDPC.SimpleEncoder()
let decoder = new OpenAir.LDPC.Decoder()

let input = Sources.Random.Make (Configuration.MAX_BLOCK_LENGTH, 13)
let configuration = Configuration.MkFromBlockLength(input.Length, 2, 3)

let channel_input = encoder.EncodeFull(input, configuration)

let start = channel_input.Start
let stop = channel_input.Start + input.Length * 8 + 284

let ideal_channel_output =
    Quantizer.Binary.Map.Mk(8, channel_input.Buffer)
    |> Array.mapi (fun  i v -> if i < start || i > stop then 0 else v)
    |> Channel.Ideal.Apply
    |> Seq.map sbyte
    |> Seq.toArray

let ideal_result = decoder.Decode(ideal_channel_output, configuration, 5, input.Length)

Array.forall2 (=) input ideal_result

