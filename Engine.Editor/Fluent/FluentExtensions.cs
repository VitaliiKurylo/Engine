using System;

namespace Engine.Editor.Fluent
{
    static class FluentExtensions
    {
        public static IFluent<TValue> Not<TValue>(this TValue @this)
        {
            return Fluent<TValue>.Resolve(@this, true);
        }

        //public static INullableFluent<TValue> IsNull<TValue>(this TValue @this, Action<TValue> action)
        //{
        //    if (ReferenceEquals(@this, null)) return NullableFluent<TValue>.Resolve();

        //    action(@this);

        //    return NullableFluent<TValue>.Resolve(@this);
        //}

        //public static INullableFluent<TValue> IsNull<TValue>(this TValue @this, Action action)
        //{
        //    if (ReferenceEquals(@this, null)) return NullableFluent<TValue>.Resolve();

        //    action();

        //    return NullableFluent<TValue>.Resolve(@this);
        //}

        public static INullableFluent<TValue> Null<TValue>(this IFluent<TValue> @this, Action<TValue> action)
        {
            if (ReferenceEquals(@this, null)) return NullableFluent<TValue>.Resolve();

            if (@this.Negation)
            {
                if (ReferenceEquals(@this.Result, null))
                    return NullableFluent<TValue>.Resolve(@this);
            }
            else
            {
                if (ReferenceEquals(@this.Result, null) == false)
                    return NullableFluent<TValue>.Resolve(@this);
            }

            action(@this.Result);

            return NullableFluent<TValue>.Resolve(@this);
        }

        public static INullableFluent<TValue> Null<TValue>(this IFluent<TValue> @this, Action action)
        {
            if (ReferenceEquals(@this, null)) return NullableFluent<TValue>.Resolve();

            if (@this.Negation)
            {
                if (ReferenceEquals(@this.Result, null))
                    return NullableFluent<TValue>.Resolve(@this);
            }
            else
            {
                if (ReferenceEquals(@this.Result, null) == false)
                    return NullableFluent<TValue>.Resolve(@this);
            }

            action();

            return NullableFluent<TValue>.Resolve(@this);
        }

        public static IFluent<TValue> Else<TValue>(this INullableFluent<TValue> @this, Action<TValue> action)
        {
            if (ReferenceEquals(@this, null)) return Fluent<TValue>.Resolve();

            if (@this.Negation)
            {
                if (ReferenceEquals(@this.Result, null) == false)
                    return @this;
            }
            else
            {
                if (ReferenceEquals(@this.Result, null))
                    return @this;
            }

            action(@this.Result);

            return @this;
        }

        public static IFluent<TValue> Else<TValue>(this INullableFluent<TValue> @this, Action action)
        {
            if (ReferenceEquals(@this, null)) return Fluent<TValue>.Resolve();

            if (@this.Negation)
            {
                if (ReferenceEquals(@this.Result, null) == false)
                    return @this;
            }
            else
            {
                if (ReferenceEquals(@this.Result, null))
                    return @this;
            }

            action();

            return @this;
        }
    }
}
