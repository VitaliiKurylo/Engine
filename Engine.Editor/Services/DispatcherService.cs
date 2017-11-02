using System;
using System.Windows.Threading;

namespace Engine.Editor.Services
{
    class DispatcherService : IDispatcherService
    {
        private readonly Dispatcher _dispatcher;
        
        public DispatcherService(Dispatcher dispatcher)
        {
            _dispatcher = dispatcher;
        }
        
        public void Invoke(Action action)
        {
            _dispatcher.Invoke(action);
        }
        
        public DispatcherOperation BeginInvoke(Action action, params object[] arguments)
        {
            return _dispatcher.BeginInvoke(action, arguments);
        }
    }
}
