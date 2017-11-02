using Engine.Editor.Attributes;
using System.Windows;

namespace Engine.Editor
{
    public partial class MainApplication
    {
        [Interceptor()]
        protected override void OnStartup(StartupEventArgs eventArguments)
        {
            base.OnStartup(eventArguments);

            var bootstrapper = new BootStrapper();
            bootstrapper.Run();
        }
    }
}
