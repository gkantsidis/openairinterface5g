namespace OpenAir.Bench

module Sources =
    open System
    open System.Collections.Generic
    open System.Diagnostics.Contracts

    type SourceException (message:string, ?innerException:exn) =
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
            raise (SourceException message)
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

    type Constant =
        static member Make (length : int, value : byte) =
            if length <= 0 then
                error "Length must be positive, it is %d; aborting operation" length
                raise (invalidArg "length" "Length must be positive")
            Contract.EndContractBlock()

            Array.init length (fun _ -> value)

        static member Set (entries : byte [], value : byte) =
            if isNull entries then
                error "Entries cannot be null array; aborting"
                raise (nullArg "entries")
            Contract.EndContractBlock()

            for i = 0 to (entries.Length - 1) do entries.[i] <- value

        static member Zero (length : int) =
            if length <= 0 then
                error "Length must be positive, it is %d; aborting operation" length
                raise (invalidArg "length" "Length must be positive")
            Contract.EndContractBlock()

            Array.init length (fun _ -> 0uy)

        static member SetZero (entries : byte []) =
            if isNull entries then
                error "Entries cannot be null array; aborting"
                raise (nullArg "entries")
            Contract.EndContractBlock()

            for i = 0 to (entries.Length - 1) do entries.[i] <- 0uy

        static member One (length : int) =
            if length <= 0 then
                error "Length must be positive, it is %d; aborting operation" length
                raise (invalidArg "length" "Length must be positive")
            Contract.EndContractBlock()

            Array.init length (fun _ -> 1uy)

        static member SetOne (entries : byte []) =
            if isNull entries then
                error "Entries cannot be null array; aborting"
                raise (nullArg "entries")
            Contract.EndContractBlock()

            for i = 0 to (entries.Length - 1) do entries.[i] <- 1uy

    type Random =
        static member Make (length : int, seed : int) =
            if length <= 0 then
                error "Length must be positive, it is %d; aborting operation" length
                raise (invalidArg "length" "Length must be positive")
            Contract.EndContractBlock()

            let rng = Random(seed)
            let result = Array.zeroCreate length
            rng.NextBytes(result)
            result

        static member Make (length : int, rng : System.Random) =
            if length <= 0 then
                error "Length must be positive, it is %d; aborting operation" length
                raise (invalidArg "length" "Length must be positive")
            Contract.EndContractBlock()

            let result = Array.zeroCreate length
            rng.NextBytes(result)
            result

        static member Set(entries : byte [], seed : int) =
            if isNull entries then
                error "Entries cannot be null array; aborting"
                raise (nullArg "entries")
            Contract.EndContractBlock()

            let rng = Random(seed)
            rng.NextBytes(entries)

        static member Set(entries : byte [], rng : System.Random) =
            if isNull entries then
                error "Entries cannot be null array; aborting"
                raise (nullArg "entries")
            Contract.EndContractBlock()
            rng.NextBytes(entries)

    type Binary =
        static member Random(length, rng : System.Random) =
            if length <= 0 then
                error "Length must be positive, it is %d; aborting operation" length
                raise (invalidArg "length" "Length must be positive")
            Contract.EndContractBlock()

            List.init length (fun _ -> if rng.NextDouble() > 0.5 then 1uy else 0uy)

        static member Random(length : int, seed : int) =
            if length <= 0 then
                error "Length must be positive, it is %d; aborting operation" length
                raise (invalidArg "length" "Length must be positive")
            Contract.EndContractBlock()

            let rng = Random(seed)
            Binary.Random(length, rng)

        static member Random(rng : System.Random) =
            Seq.initInfinite (fun _ -> if rng.NextDouble() > 0.5 then 1uy else 0uy)

        static member RandomSeq(seed : int) =
            let rng = System.Random(seed)
            Binary.Random(rng)

        static member Alternate(length) =
            if length <= 0 then
                error "Length must be positive, it is %d; aborting operation" length
                raise (invalidArg "length" "Length must be positive")
            Contract.EndContractBlock()

            List.init length (fun i -> if i % 2 = 0 then 0uy else 1uy)

        static member Alternate() =
            Seq.initInfinite (fun i -> if i % 2 = 0 then 0uy else 1uy)
