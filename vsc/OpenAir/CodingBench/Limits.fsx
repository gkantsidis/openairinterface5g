#load "Init.fsx"

open System
open System.IO
open OpenAir.LDPC
open OpenAir.Bench

open Deedle

type BinaryGaussianProfile  =
    { SNR : float; UncodedBer : float; Capacity : float; ShannonCapacity : float }

let binary_gaussian_error_capacity_profile : Deedle.Frame<int, string> =
    [-2.0..0.01..8.0]
    |> List.map snr
    |> List.map (
        fun snr ->
            {
                SNR             = snr.value
                UncodedBer      = Channel.Gaussian.Binary.Error snr
                Capacity        = Channel.Gaussian.Binary.Capacity snr
                ShannonCapacity = Channel.Gaussian.capacity snr
            }
    )
    |> Frame.ofRecords


let mkFile filename =
    Path.Combine(__SOURCE_DIRECTORY__, "Results", filename)

let save frame filename =
    let name = FileInfo(filename)
    let filename' = if String.IsNullOrWhiteSpace(name.Extension) then sprintf "%s.csv" filename else filename
    let full = mkFile filename'
    FrameExtensions.SaveCsv(frame, full, culture=System.Globalization.CultureInfo.InvariantCulture)

save binary_gaussian_error_capacity_profile "ideal"

type SnrForRateProfile =
    { Rate : float; SNR : float}

let binary_gaussian_snr_for_rate =
    [ 0.75..0.001..0.99 ]
    |> List.map (
        fun rate ->
            let snr = Channel.Gaussian.Binary.SnrFromCodeRate rate
            { Rate = rate; SNR = snr.value }
    ) |> Frame.ofRecords

save binary_gaussian_snr_for_rate "snr_for_rate_binary_awgn"

