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

let ideal_channel (input : System.Collections.Generic.IReadOnlyList<byte>, output : System.Collections.Generic.IList<sbyte>) =
    assert (input.Count = output.Count)

    let bits_to_shift = 8 - 1
    let maxlev = 1 <<< bits_to_shift

    for i = 0 to (input.Count - 1) do
         let level = if input.[i] = 0uy then maxlev - 1 else -maxlev
         output.[i] <- sbyte(level)

let data = mk_random_data 13 (8448/8)
// let data = mk_const_data 1uy (8448/8)
let configuration = OpenAir.LDPC.Configuration.MkFromBlockLength(data.Length * 8)

let channel_in = encoder.Encode(data, configuration)
let channel_slice = configuration.SliceInputToChannel(channel_in, data.Length)
let channel_out = OpenAir.LDPC.SimpleEncoder.GetChannelOutputBuffer()

let channel_out_slice = configuration.SliceOutputFromChannel(channel_out, data.Length)

ideal_channel (channel_slice, channel_out_slice)

let result = decoder.Decode(channel_out, configuration, 5, data.Length * 8)

let inline eq x y = x = y
Array.forall2 eq data result
