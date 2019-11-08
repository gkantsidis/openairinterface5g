namespace OpenAir.Bench

[<AutoOpen>]
module Metrics =
    open System

    [<StructuredFormatDisplay("{Display")>]
    type SNR = | SNR of float
    with
        member this.value  = let (SNR value) = this in value
        member this.linear = Math.Pow(10.0, this.value / 10.0)
        member this.Display = sprintf "%6.3f dB" this.value
        override this.ToString() = this.Display

    let inline snr value = SNR value
