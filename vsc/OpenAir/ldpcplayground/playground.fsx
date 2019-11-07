#I @"../../.paket/load/net472"
#load "MathNet.Numerics.fsx"

open System

let [<Literal>] TOP_DIRECTORY = __SOURCE_DIRECTORY__ + @"\.."
let [<Literal>] BINARIES = TOP_DIRECTORY + @"\x64\Debug"
let [<Literal>] OPEN_AIR_LIBRARY = BINARIES + "@\OpenAir.NET.dll"

#I @"..\x64\Debug\"
#r "OpenAir.NET.dll"

module Array =
    let inline set_all<'T> (v : 'T) (arr : 'T []) =
        for i = 0 to (arr.Length-1) do arr.[i] <- v

let encoder = OpenAir.LDPC.SimpleEncoder()
let decoder = new OpenAir.LDPC.Decoder()

let mk_random_data seed size =
    let rng = System.Random(seed)
    let data = Array.zeroCreate size
    rng.NextBytes(data)
    data

let mk_const_data (value : byte) size = Array.init size (fun _ -> value)

let ideal_channel_binary (qbits : int) (input : System.Collections.Generic.IReadOnlyList<byte>, output : System.Collections.Generic.IList<sbyte>) =
    assert (input.Count = output.Count)

    let bits_to_shift = qbits - 1
    let maxlev = 1 <<< bits_to_shift

    for i = 0 to (input.Count - 1) do
         let level = if input.[i] = 0uy then maxlev - 1 else -maxlev
         output.[i] <- sbyte(level)

let length = OpenAir.LDPC.Configuration.MAX_BLOCK_LENGTH
let data = mk_random_data 13 length
// let data = mk_const_data 1uy length
// let configuration = OpenAir.LDPC.Configuration.MkFromBlockLength(data.Length * 8)
let configuration = OpenAir.LDPC.Configuration.MkFromBlockLength(data.Length, 1, 3)

let channel = encoder.EncodeFull(data, configuration)



let inline mk_linear_snr snr = Math.Pow(10.0, snr / 10.0)




let channel_in = encoder.Encode(data, configuration)
let channel_slice = configuration.SliceInputToChannel(channel_in, data.Length)
let channel_out = OpenAir.LDPC.SimpleEncoder.GetChannelOutputBuffer()

let channel_out_slice = configuration.SliceOutputFromChannel(channel_out, data.Length)

ideal_channel_binary 8 (channel_slice, channel_out_slice)

let result = decoder.Decode(channel_out, configuration, 5, data.Length)

let inline eq x y = x = y
Array.forall2 eq data result


mk_linear_snr 6.0

open MathNet.Numerics.Distributions
let add_gaussian_noise (rng : System.Random) (snr : float) (input : System.Collections.Generic.IReadOnlyList<byte>) : float seq =
    let snr_linear = Math.Pow(10.0, snr / 10.0)
    let sigma = 1.0 / Math.Sqrt(2.0 * snr_linear)

    let gaussian = MathNet.Numerics.Distributions.Normal(0.0, 1.0, rng)
    let samples = gaussian.Samples()
    Seq.map2 (
        fun value noise ->
            let v = if value = 1uy then -1.0 elif value = 0uy then 1.0 else failwithf "Illegal value %d" value
            let nom = v + sigma * noise
            let denom = sigma / (4.0 * 4.0)
            nom / denom

    ) input samples

let quantize (qbits : int) (input : System.Collections.Generic.IReadOnlyList<float>) =
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

let copy (input : System.Collections.Generic.IReadOnlyList<sbyte>, output : System.Collections.Generic.IList<sbyte>) =
    assert (input.Count = output.Count)
    for i = 0 to (input.Count - 1) do
        output.[i] <- input.[i]


let rec count_bits (number : byte) =
    if number = 0uy then 0
    else 1 + count_bits (number &&& (number - 1uy))

let rng = System.Random(13)
let xxx = add_gaussian_noise rng 4.0 channel_slice |> Seq.toList |> quantize 8 |> Seq.toList
channel_out.Initialize()
copy (xxx, channel_out_slice)
let r2 = decoder.Decode(channel_out, configuration, 5, data.Length * 8)
Array.forall2 eq data r2

let output_bit_errors =
    Seq.map2 (
        fun (l:byte) (r:byte) ->
            count_bits (l ^^^ r)
    ) data r2
    |> Seq.sum

let channel_bit_errors =
    Seq.map2 (
        fun (l : byte) (r : sbyte) ->
            if r >= 0y && l = 1uy   then 1
            elif r < 0y  && l = 0uy then 1
            elif l <> 0uy && l <> 1uy then failwith "error"
            else 0
    ) channel_slice channel_out_slice
    |> Seq.sum

let total_bits = data.Length * 8
let uncoded_bit_error_rate = 100.0 * float(channel_bit_errors) / float(channel_out_slice.Count)


let efficiency = float(channel_slice.Count)

let quantize_to_normalized_channel_2 (input : System.Collections.Generic.IReadOnlyList<byte>) =
    let map (b1: byte) (b2: byte) =
        match b1, b2 with
        | 0uy, 0uy -> 1.0
        | 0uy, 1uy -> 1.0/3.0
        | 1uy, 1uy -> -1.0/3.0
        | 1uy, 0uy -> -1.0
        | _, _ -> failwithf "Invalid value"

    let length = Math.Ceiling (float(input.Count) / 2.0) |> int
    List.init length (
        fun i ->
            let b1 = input.Item (2 * i)
            let b2 = if (2 * i + 1) >= input.Count then 0uy else input.Item (2 * i + 1)
            map b1 b2
    )

let add_channel_gaussian_noise (rng : System.Random) (snr : float) (input : float list) : float list =
    let snr_linear = Math.Pow(10.0, snr / 10.0)
    let sigma = 1.0 / Math.Sqrt(2.0 * snr_linear)
    let gaussian = MathNet.Numerics.Distributions.Normal(0.0, 1.0, rng)

    input
    |> List.map (
        fun value ->
            value + sigma * gaussian.Sample()
    )

let y1 = quantize_to_normalized_channel_2 channel_slice
let y2 = add_channel_gaussian_noise rng 4.0 y1
