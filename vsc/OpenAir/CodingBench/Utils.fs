namespace OpenAir.Bench

[<AutoOpen>]
module Utils =
    open System

    let inline entropy p = -p * Math.Log(p, 2.0) - (1.0 - p) * Math.Log(1.0 - p, 2.0)

    /// An upper bound of the inverse of the entropy function
    /// (taken from [http://cseweb.ucsd.edu/~ccalabro/thesis.pdf, p.9].
    /// This bound is not usually very useful.
    let inverse_entropy_upper (x : float) = x / Math.Log(1.0 / x, 2.0)

    /// A lower bound of the inverse of the entropy function
    /// (taken from [http://cseweb.ucsd.edu/~ccalabro/thesis.pdf, p.9]
    let inverse_entropy_lower (x : float) = x / (2.0 * Math.Log(6.0 / x, 2.0))

    /// Approximates the inverse of the entropy function.
    /// The result is in the range (0, 1/2)
    let inverse_entropy y =
        let estimate : Func<float, float> =  Func<float, float>(fun x -> (entropy x) - y)
        let derivative : Func<float, float> =  Func<float, float> (fun x -> Math.Log(1.0 - x, 2.0) - Math.Log(x, 2.0))
        let lower = inverse_entropy_lower y
        MathNet.Numerics.RootFinding.RobustNewtonRaphson.FindRoot(estimate, derivative, lower, 0.5)
    
