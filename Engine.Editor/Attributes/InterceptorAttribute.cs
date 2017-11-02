using System;
using System.Diagnostics;
using System.Reflection;

namespace Engine.Editor.Attributes
{
    [AttributeUsage(AttributeTargets.Method | AttributeTargets.Constructor | AttributeTargets.Assembly | AttributeTargets.Module)]
    class InterceptorAttribute : Attribute, IMethodDecorator
    {
        public void OnEntry(MethodBase method)
        {
            Debug.WriteLine($"OnEntry: {method.DeclaringType.FullName + "." + method.Name}");
        }
        
        public void OnExit(MethodBase method)
        {
            Debug.WriteLine($"OnExit: {method.DeclaringType.FullName + "." + method.Name}");
        }
        
        public void OnException(MethodBase method, Exception exception)
        {
            Debug.WriteLine($"OnException: {method.DeclaringType.FullName + "." + method.Name}{Environment.NewLine + exception.GetType()}: {exception.Message}");
        }
    }
}
