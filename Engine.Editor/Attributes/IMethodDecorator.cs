using System;
using System.Reflection;

interface IMethodDecorator
{
    /// <summary>
    ///
    /// </summary>
    void OnEntry(MethodBase method);

    /// <summary>
    ///
    /// </summary>
    void OnExit(MethodBase method);

    /// <summary>
    ///
    /// </summary>
    /// <param name="exception"></param>
    void OnException(MethodBase method, Exception exception);
}
