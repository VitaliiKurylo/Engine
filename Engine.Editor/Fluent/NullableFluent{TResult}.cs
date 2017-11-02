namespace Engine.Editor.Fluent
{
    interface INullableFluent<TResult> : IFluent<TResult> { }
    
    class NullableFluent<TResult> : Fluent<TResult>, INullableFluent<TResult>
    {
        protected NullableFluent(TResult result, bool negation)
            : base(result, negation)
        {
        }
        
        public static new INullableFluent<TResult> Resolve(TResult result = default(TResult), bool negation = false)
        {
            return new NullableFluent<TResult>(result, negation);
        }
        
        public static INullableFluent<TResult> Resolve(IFluent<TResult> result)
        {
            return new NullableFluent<TResult>(result.Result, result.Negation);
        }
    }
}
