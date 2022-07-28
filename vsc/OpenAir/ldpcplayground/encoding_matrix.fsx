#r "nuget: HDF.PInvoke.NETStandard, 1.10.502"
#I @"..\x64\Debug\"
#r "OpenAir.NET.dll"

open System
open System.IO

let encoder = OpenAir.LDPC.SimpleEncoder()

let length = OpenAir.LDPC.Configuration.MAX_BLOCK_LENGTH
let buffer : byte[] = Array.zeroCreate length

let configuration = OpenAir.LDPC.Configuration.MkFromBlockLength(buffer.Length, 1, 3)
let channel = encoder.EncodeFull(buffer, configuration)

module Array =
    let inline set_all<'T> (v : 'T) (arr : 'T []) =
        for i = 0 to (arr.Length-1) do arr.[i] <- v

    let find_non_zero (buffer : byte[]) =
        let indices = buffer |> Array.mapi (fun i x -> if x <> 0uy then Some (i, x) else None) |> Array.choose id
        if indices |> Array.exists (fun (_, v) -> v <> 1uy) then
            failwithf "Error: found unexpected code"
        indices |> Array.map fst
    

Array.forall ((=) 0uy) channel.Buffer
buffer.[0] <- 0x01uy

let get_coefficients buffer i =
    Array.set_all 0uy buffer

    let byte_index = i / 8
    let offset = i % 8

    let v = 1uy <<< offset
    buffer.[byte_index] <- v

    let result = encoder.EncodeFull(buffer, configuration)
    Array.find_non_zero result.Buffer


let coefs = get_coefficients buffer

let dependencies : bool[,] = Array2D.zeroCreate (8*length) (channel.Length)
for i in 0 .. (8*length - 1) do
    coefs i
    |> Array.iter (
        fun v ->            
            dependencies.[i, v] <- true
    )

let col_nnz (buffer: bool[,]) col =
    let n = Array2D.length1 buffer
    let mutable sum = 0
    for i in 0 .. (n-1) do
        if buffer.[i, col] then
            sum <- sum + 1
    sum

let row_nnz (buffer: bool[,]) row =
    let n = Array2D.length2 buffer
    let mutable sum = 0
    for i in 0 .. (n-1) do
        if buffer.[row, i] then
            sum <- sum + 1
    sum

let checksum_degrees =
    let checksums = Array2D.length2 dependencies
    Array.init checksums (fun i -> col_nnz dependencies i)

let row_degrees =
    let bits = Array2D.length1 dependencies
    Array.init bits (fun i -> row_nnz dependencies i)

let write_to_json (writer: TextWriter) =    
    Printf.fprintfn writer "{"

    Printf.fprintfn writer "\t\"Length\": %d," length
    Printf.fprintfn writer "\t\"InputBits\": %d," (8*length)
    Printf.fprintfn writer "\t\"NumberOfChecksum\": %d," (channel.Length)

    Printf.fprintfn writer "\t\"Dependencies\": ["
    for information_bit in 0 .. (Array2D.length1 dependencies - 1) do
        Printf.fprintfn writer "\t\t{ \"Bit\": %d," information_bit
        Printf.fprintfn writer "\t\t  \"Dependencies\": ["

        [0 .. (Array2D.length2 dependencies - 1)]
        |> Seq.filter (fun checksum_bit -> dependencies.[information_bit, checksum_bit])
        |> Seq.map (sprintf "%d")
        |> String.concat ",\n\t\t\t"
        |> Printf.fprintf writer "%s"

        //for checksum_bit in 0 .. (Array2D.length2 dependencies - 1) do
        //    if dependencies.[information_bit, checksum_bit] then
        //        Printf.fprintfn writer "\t\t\t%d," checksum_bit

        Printf.fprintfn writer "\t\t]"
        if information_bit = (Array2D.length1 dependencies - 1) then
            Printf.fprintfn writer "\t\t}"
        else
            Printf.fprintfn writer "\t\t},"
        
    Printf.fprintfn writer "\t],"

    Printf.fprintfn writer "\t\"InformationBitDegree\": ["
    row_degrees |> Seq.map (sprintf "%d") |> String.concat ",\n\t\t" |> Printf.fprintfn writer "%s"
    Printf.fprintfn writer "\t],"

    Printf.fprintfn writer "\t\"ChecksumDegree\": ["
    checksum_degrees |> Seq.map (sprintf "%d") |> String.concat ",\n\t\t" |> Printf.fprintfn writer "%s"
    Printf.fprintfn writer "\t]"

    Printf.fprintfn writer "}"

let write (filename: string) =
    use writer = File.CreateText filename
    write_to_json writer

write "e:\\temp\\code.json"


open HDF.PInvoke

let fid = H5F.create("e:\\temp\\code.hdf5", H5F.ACC_TRUNC, H5P.DEFAULT, H5P.DEFAULT)

//let f_info_degree = H5D.create(fid, "dinfobits" )


H5F.close(fid)
