using System;
using System.Windows.Threading;

namespace Engine.Editor.Services
{
    interface IDispatcherService
    {
        void Invoke(Action action);
        
        DispatcherOperation BeginInvoke(Action action, params object[] arguments);
    }
}
