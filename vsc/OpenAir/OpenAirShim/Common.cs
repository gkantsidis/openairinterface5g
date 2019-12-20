namespace OpenAirShim
{
    public enum BaseGraph
    {
        BG1 = 1,
        BG2 = 2
    }

    [System.Serializable]
    public class OpenAirShimException : System.Exception
    {
        public OpenAirShimException() { }
        public OpenAirShimException(string message) : base(message) { }
        public OpenAirShimException(string message, System.Exception inner) : base(message, inner) { }
        protected OpenAirShimException(
          System.Runtime.Serialization.SerializationInfo info,
          System.Runtime.Serialization.StreamingContext context) : base(info, context) { }
    }
}
