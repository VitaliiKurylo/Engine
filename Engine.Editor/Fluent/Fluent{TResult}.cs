namespace Engine.Editor.Fluent
{
    interface IFluent<TResult>
    {
        TResult Result { get; }
        
        bool Negation { get; }
    }
    
    class Fluent<TResult> : IFluent<TResult>
    {
        protected Fluent(TResult result, bool negation)
        {
            Result = result;
            Negation = negation;
        }
        
        public static IFluent<TResult> Resolve(TResult result = default(TResult), bool negation = false)
        {
            return new Fluent<TResult>(result, negation);
        }
        
        public TResult Result { get; private set; }
        
        public bool Negation { get; private set; }
    }
}
