#load "Init.fsx"

open OpenAir.LDPC
open OpenAir.Bench

module LDPC =
    open System.Runtime.InteropServices

    let [<Literal>] BINARY_PATH = __SOURCE_DIRECTORY__ + @"\..\x64\Debug"
    let [<Literal>] LIBRARY = BINARY_PATH + @"\OpenAirDll.dll"

    [<DllImport(LIBRARY, EntryPoint = "ldpc_encode_simple", CallingConvention = CallingConvention.Cdecl)>]
    extern int encode(voidptr input, int input_length, voidptr encoded, int base_graph);

let input = Sources.Random.Make (Configuration.MAX_BLOCK_LENGTH, 13)
let input_buffer = System.Memory(input)
let input_handle = input_buffer.Pin()
let input_pinned = input_handle.Pointer

let output : byte [] = Array.zeroCreate (68 * 384)
let output_buffer = System.Memory(output)
let output_handle = output_buffer.Pin()
let output_pinned = output_handle.Pointer

let result = LDPC.encode(input_pinned, input.Length, output_pinned, 2)

