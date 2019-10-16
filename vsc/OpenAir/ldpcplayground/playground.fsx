let [<Literal>] TOP_DIRECTORY = __SOURCE_DIRECTORY__ + @"\.."
let [<Literal>] BINARIES = TOP_DIRECTORY + @"\x64\Debug"
let [<Literal>] OPEN_AIR_LIBRARY = BINARIES + "@\OpenAir.NET.dll"

#I @"..\x64\Debug\"
#r "OpenAir.NET.dll"

let encoder = OpenAir.LDPC.Encoder()
let decoder = new OpenAir.LDPC.Decoder()

