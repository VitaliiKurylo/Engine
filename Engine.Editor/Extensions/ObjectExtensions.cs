using System;
using System.Linq.Expressions;

namespace Engine.Editor.Extensions
{
    static class ObjectExtensions
    {
        public static TProperty GetValue<TProperty, TClass>(this Object @this, Expression<Func<TClass, TProperty>> action) where TClass : class
        {
            return (TProperty)typeof(TClass).GetProperty(@this.GetPropertyName(action)).GetValue(@this);
        }

        public static string GetPropertyName<TProperty, TClass>(this Object @this, Expression<Func<TClass, TProperty>> action) where TClass : class
        {
            var expression = (MemberExpression)action.Body;
            return expression.Member.Name;
        }
    }
}
