#I @"../../.paket/load/net472"
#load "MathNet.Numerics.fsx"
#load "NLog.fsx"
#load "FSharp.Collections.ParallelSeq.fsx"
#load "R.NET.fsx"
#load "R.NET.FSharp.fsx"
#load "R.NET.Community.fsx"
#load "R.NET.Community.FSharp.fsx"

#I @"../../packages"

// #load "Deedle.fsx"
#r "Deedle/lib/net45/Deedle.dll"
do fsi.AddPrinter(fun (printer:Deedle.Internal.IFsiFormattable) -> "\n" + (printer.Format()))
open Deedle
open Deedle.``F# Frame extensions``


#I @"..\x64\Debug\"
#r "OpenAir.NET.dll"

#load "Errors.fs"
#load "Metrics.fs"
#load "Utils.fs"
#load "Channels.fs"
#load "Quantizer.fs"
#load "Sources.fs"
