#load "Init.fsx"

open OpenAir.LDPC
open OpenAir.Bench

open Deedle.Frame

type BinaryGaussianProfile  =
    { UncodedBer : float; Capacity : float; ShannonCapacity : float }

let binary_gaussian_error_capacity_profile =
    [3.0..0.01..8.0]
    |> List.map snr
    |> List.map (
        fun snr ->
            {
                UncodedBer      = Channel.Gaussian.Binary.Error snr
                Capacity        = Channel.Gaussian.Binary.Capacity snr
                ShannonCapacity = Channel.Gaussian.capacity snr
            }
    )
    |> Deedle.Frame.ofRecords

