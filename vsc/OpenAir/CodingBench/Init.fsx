#I @"../../.paket/load/net472"
#load "MathNet.Numerics.fsx"
#load "NLog.fsx"



#I @"../../packages"

// #load "Deedle.fsx"
#r "Deedle/lib/net45/Deedle.dll"
do fsi.AddPrinter(fun (printer:Deedle.Internal.IFsiFormattable) -> "\n" + (printer.Format()))
open Deedle
open Deedle.``F# Frame extensions``

// #load "RProvider.fsx"
#load "DynamicInterop.fsx" 
#load "R.NET.Community.fsx" 
#load "R.NET.Community.FSharp.fsx" 
#r "RProvider/lib/net40/RProvider.Runtime.dll" 
#r "RProvider/lib/net40/RProvider.dll" 

#I @"../../packages/RProvider/lib/net40"
#r "DynamicInterop.dll"
#r "RDotNet.dll"
#r "RDotNet.FSharp.dll"
#r "RProvider.dll"
#r "RProvider.Runtime.dll"

open RProvider
do fsi.AddPrinter(fun (synexpr:RDotNet.SymbolicExpression) -> synexpr.Print())


#I @"..\x64\Debug\"
#r "OpenAir.NET.dll"

#load "Errors.fs"
#load "Metrics.fs"
#load "Utils.fs"
#load "Channels.fs"
#load "Quantizer.fs"
#load "Sources.fs"
