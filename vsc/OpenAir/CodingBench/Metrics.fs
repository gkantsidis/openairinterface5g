namespace OpenAir.Bench

[<AutoOpen>]
module Metrics =
    open System

    [<StructuredFormatDisplay("{AsString")>]
    type SNR = | SNR of float
    with
        member this.value  = let (SNR value) = this in value
        member this.linear = Math.Pow(10.0, this.value / 10.0)
        member this.AsString = sprintf "%6.3f dB" this.value
        override this.ToString() = this.AsString

    let inline snr value = SNR value

    /// Compute the SNR that would give error with the given
    /// standard deviation sigma in a gaussian channel.
    let snr_from_sigma (sigma : float) =
        let ratio = 1.0 / sigma
        let linear = 0.5 * ratio * ratio
        10.0 * Math.Log10(linear) |> snr
