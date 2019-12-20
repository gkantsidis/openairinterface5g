namespace OpenAir.Bench

module Quantizer =
    open System.Collections.Generic
    open System.Diagnostics.Contracts

    type QuantizerException (message:string, ?innerException:exn) =
        inherit BenchException(
            message,
            match innerException with | Some ex -> ex | _ -> null)

    #if DONT_USE_NLOG
    let inline private display_message prefix message = Printf.printfn "[%s]: %s" prefix message
    let inline private throw fmt    = failwithf fmt
    let inline private warn fmt     = Printf.ksprintf  (display_message "WARNING") fmt
    let inline private debug fmt    = Printf.ksprintf (display_message "DEBUG") fmt
    let inline private error fmt    = Printf.ksprintf (fun msg -> raise (GraphsException msg)) fmt
    #else
    open NLog

    /// Logger for this module
    let private _logger = LogManager.GetCurrentClassLogger()

    let inline private throw fmt =
        let do_throw (message : string) =
            _logger.Error message
            raise (QuantizerException message)
        Printf.ksprintf do_throw fmt

    let inline private warn fmt = Printf.ksprintf _logger.Warn fmt
    let inline private debug fmt = Printf.ksprintf _logger.Debug fmt
    let inline private error fmt = Printf.ksprintf _logger.Error fmt

    #if INTERACTIVE
    // The following are used only in interactive (fsi) to help with enabling disabling
    // logging for particular modules.

    type internal Marker = interface end
    let _full_name = typeof<Marker>.DeclaringType.FullName
    let _name = typeof<Marker>.DeclaringType.Name
    #endif
    #endif

    module Binary =
        let private from_bool (v : bool) = if v then 1 else 0
        let private to_bool (v : int) = if v >= 0 then false else true

        let private map =
            function
            | 0     -> 1.0
            | 1     -> -1.0
            | v     -> throw "Invalid value %d for binary channel" v

        let private inv value = if value >= 0.0 then 0 else 1

        let private map_q (levels : int) value =
            assert (levels > 0)
            let bits_to_shift = levels - 1
            let maxlev = 1 <<< bits_to_shift

            if value = 1 then -maxlev
            elif value = 0 then maxlev - 1
            else throw "Invalid value %d for binary channel" value

        type Map =
            static member Mk (input : bool list) = input |> List.map (from_bool >> map)
            static member Mk (input : bool seq) = input |> Seq.map (from_bool >> map)
            static member Mk (input : bool []) = input |> Array.map (from_bool >> map)
            static member Mk (input : IReadOnlyList<bool>) : IList<float> =
                let result = List.init input.Count (fun i -> input.Item i |> from_bool |> map)
                List(result) :> IList<float>

            static member Mk (input : int list) = input |> List.map map
            static member Mk (input : int seq) = input |> Seq.map map
            static member Mk (input : int []) = input |> Array.map map
            static member Mk (input : byte []) = input |> Array.map (int >> map)

            static member Mk (levels : int, input : bool list) =
                if levels <= 0 then
                    error "Levels should be positive, it is %d; aborting operation" levels
                    raise (invalidArg "levels" "Levers should be positive")
                Contract.EndContractBlock()

                input |> List.map (from_bool >> map_q levels)

            static member Mk (levels : int, input : bool seq) =
                if levels <= 0 then
                    error "Levels should be positive, it is %d; aborting operation" levels
                    raise (invalidArg "levels" "Levers should be positive")
                Contract.EndContractBlock()
                input |> Seq.map (from_bool >> map_q levels)

            static member Mk (levels : int, input : IReadOnlyList<bool>) : IList<int> =
                if levels <= 0 then
                    error "Levels should be positive, it is %d; aborting operation" levels
                    raise (invalidArg "levels" "Levers should be positive")
                Contract.EndContractBlock()

                let result = List.init input.Count (fun i -> input.Item i |> from_bool |> map_q levels)
                List(result) :> IList<int>

            static member Mk (levels : int, input : int list) =
                if levels <= 0 then
                    error "Levels should be positive, it is %d; aborting operation" levels
                    raise (invalidArg "levels" "Levers should be positive")
                Contract.EndContractBlock()
                input |> List.map (map_q levels)

            static member Mk (levels : int, input : int seq) =
                if levels <= 0 then
                    error "Levels should be positive, it is %d; aborting operation" levels
                    raise (invalidArg "levels" "Levers should be positive")
                Contract.EndContractBlock()
                input |> Seq.map (map_q levels)

            static member Mk (levels : int, input : int []) =
                if levels <= 0 then
                    error "Levels should be positive, it is %d; aborting operation" levels
                    raise (invalidArg "levels" "Levers should be positive")
                Contract.EndContractBlock()
                input |> Array.map (map_q levels)

            static member Mk (levels : int, input : byte []) =
                if levels <= 0 then
                    error "Levels should be positive, it is %d; aborting operation" levels
                    raise (invalidArg "levels" "Levers should be positive")
                Contract.EndContractBlock()
                input |> Array.map (int >> map_q levels)

            static member Apply (input : IReadOnlyList<byte>, output : IList<sbyte>) =
                if isNull input then
                    error "Input vector should not be null; aborting operation"
                    raise (nullArg "input")
                if isNull output then
                    error "Output vector should not be null; aborting operation"
                    raise (nullArg "output")
                if input.Count <> output.Count then
                    error "Input and output vectors should have same length, they have %d %d; aborting operation"
                        input.Count output.Count
                    raise (invalidArg "output" "Should have same length as input")
                Contract.EndContractBlock()

                for i = 0 to (input.Count - 1) do
                    output.[i] <- input.[i] |> int |> map |> sbyte

            static member Apply (levels : int, input : IReadOnlyList<byte>, output : IList<sbyte>) =
                if isNull input then
                    error "Input vector should not be null; aborting operation"
                    raise (nullArg "input")
                if isNull output then
                    error "Output vector should not be null; aborting operation"
                    raise (nullArg "output")
                if input.Count <> output.Count then
                    error "Input and output vectors should have same length, they have %d %d; aborting operation"
                        input.Count output.Count
                    raise (invalidArg "output" "Should have same length as input")
                if levels <= 0 then
                    error "Levels should be positive, it is %d; aborting operation" levels
                    raise (invalidArg "levels" "Levers should be positive")
                Contract.EndContractBlock()

                for i = 0 to (input.Count - 1) do
                    output.[i] <- input.[i] |> int |> map_q levels |> sbyte

            static member Inv (input : float list) = input |> List.map inv
            static member Inv (input : float seq) = input |> Seq.map inv
            static member Inv (input : IReadOnlyList<float>) : IList<int> =
                let result = List.init input.Count (fun i -> input.Item i |> inv)
                List(result) :> IList<int>

            module Array =
                let map_byte_in_place (output : float []) (input : byte []) =
                    if output.Length <> input.Length then
                        error "Arrays must have same length, they have %d <> %d; exiting"
                            output.Length input.Length
                        raise (invalidArg "output" "Invalid length")
                    Contract.EndContractBlock()

                    let len = input.Length

                    for i in 0 .. (len - 1) do
                        output.[i] <- int(input.[i]) |> map

                    output

    let private quantize_f_q (levels : int) value =
        assert (levels > 0)
        let bits_to_shift = levels - 1
        let maxlev = (1 <<< bits_to_shift) |> float
        let level =
            if value <= -maxlev     then -maxlev
            elif value >= maxlev    then maxlev - 1.0
            else value
        sbyte(level)

    module Seq =
        let quantize levels (input : float seq) = input |> Seq.map (quantize_f_q levels)

    module List =
        let quantize levels (input : float list) = input |> List.map (quantize_f_q levels)

    module Array =
        let quantize_in_place levels (output : sbyte []) (input : float []) =
            if output.Length <> input.Length then
                error "Arrays must have same length, they have %d <> %d; exiting"
                    output.Length input.Length
                raise (invalidArg "output" "Invalid length")
            Contract.EndContractBlock()

            for i in 0 .. (input.Length - 1) do
                output.[i] <- quantize_f_q levels input.[i]

            output
