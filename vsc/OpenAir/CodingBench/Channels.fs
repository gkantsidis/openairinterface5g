namespace OpenAir.Bench

module Channel =
    open System
    open System.Collections.Generic
    open System.Diagnostics.Contracts
    open Metrics
    open Utils

    type ChannelException (message:string, ?innerException:exn) =
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
            raise (ChannelException message)
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

    type Ideal =
        static member Apply<'T> (input : IReadOnlyList<'T>, output : IList<'T>) =
            if input.Count <> output.Count then
                let message = Printf.sprintf "The channels must be of same length, they are %d and %d" input.Count output.Count
                raise (invalidArg "output" message)

            if isNull input then raise (nullArg "input")
            if isNull output then raise (nullArg "output")

            Contract.EndContractBlock()

            for i = 0 to (input.Count - 1) do
                output.[i] <- input.[i]

        static member Apply<'T> (input : IReadOnlyList<'T>) : IList<'T> =
            if isNull input then raise (nullArg "input")
            Contract.EndContractBlock()
            List(input) :> IList<'T>

        static member Apply<'T> (input : 'T list) : 'T list = input

        static member Apply<'T> (input : 'T seq) : 'T seq = input

    module Gaussian =
        open MathNet.Numerics.Distributions

        type Binary =
            static member Error (snr : SNR) =
                let sigma = 1.0 / Math.Sqrt(2.0 * snr.linear)
                // Seed does not matter here
                let gaussian = Normal(0.0, sigma, Random(12))
                1.0 - gaussian.CumulativeDistribution(1.0)

            static member Capacity (snr : SNR) =
                let p = Binary.Error snr
                let h = entropy p
                1.0 - h

            static member SnrFromCodeRate (rate : float) =
               let target_entropy = 1.0 - rate
               let target_error = inverse_entropy target_entropy
               let target_cdf = 1.0 - target_error
               let target_erf = 2.0 * target_cdf - 1.0
               let target_ratio = MathNet.Numerics.SpecialFunctions.ErfInv(target_erf)
               let sigma = 1.0 / (target_ratio * Math.Sqrt(2.0))
               snr_from_sigma sigma

        let inline capacity (snr : SNR) = Math.Log(1.0 + snr.linear, 2.0)

        type Noise =
            static member Add(rng: System.Random, snr: SNR, input: float list) : float list=
                let sigma = 1.0 / Math.Sqrt(2.0 * snr.linear)
                let gaussian = Normal(0.0, sigma, rng)
                input |> List.map(fun value -> value + gaussian.Sample())

            static member Add(seed, snr: SNR, input: float list) =
                let rng = Random(seed)
                Noise.Add(rng, snr, input)

            static member Add(rng: System.Random, snr: SNR, input: float seq) : float seq=
                let sigma = 1.0 / Math.Sqrt(2.0 * snr.linear)
                let gaussian = Normal(0.0, sigma, rng)
                input |> Seq.map(fun value -> value + gaussian.Sample())

            static member Add(rng: System.Random, snr: SNR, input: IReadOnlyList<float>) : IReadOnlyList<float> =
                if isNull input then
                    error "Input vector should not be null; aborting"
                    raise (nullArg "input")
                Contract.EndContractBlock()
                Contract.Ensures((isNull (Contract.Result<IReadOnlyList<float>>())) = false)

                let sigma = 1.0 / Math.Sqrt(2.0 * snr.linear)
                let gaussian = Normal(0.0, sigma, rng)
                let result = input |> Seq.map(fun value -> value + gaussian.Sample())
                (result |> Seq.toList) :> IReadOnlyList<float>

            static member MapL(rng: System.Random, snr: SNR) =
                let sigma = 1.0 / Math.Sqrt(2.0 * snr.linear)
                let gaussian = Normal(0.0, sigma, rng)
                List.map(fun value -> value + gaussian.Sample())

            static member MapL(seed, snr: SNR) =
                let rng = Random(seed)
                Noise.MapL(rng, snr)

            static member Map(rng: System.Random, snr: SNR) =
                let sigma = 1.0 / Math.Sqrt(2.0 * snr.linear)
                let gaussian = Normal(0.0, sigma, rng)
                Seq.map(fun value -> value + gaussian.Sample())

            static member Map(seed, snr: SNR) =
                let rng = Random(seed)
                Noise.Map(rng, snr)

        module Array =
            let add (rng: System.Random, snr: SNR) (input : float []) =
                let sigma = 1.0 / Math.Sqrt(2.0 * snr.linear)
                let gaussian = Normal(0.0, sigma, rng)
                let output : float [] = Array.zeroCreate input.Length
                gaussian.Samples(output)

                Array.iteri (fun i v -> output.[i] <- v + input.[i]) output

                output

            let add_in_place (rng: System.Random, snr: SNR) (input : float []) =
                let sigma = 1.0 / Math.Sqrt(2.0 * snr.linear)
                let gaussian = Normal(0.0, sigma, rng)
                input |> Array.iteri (fun i v -> input.[i] <- v + gaussian.Sample())
                input
