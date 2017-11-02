using Engine.Editor.Views;
using Prism.Unity;
using System.Windows;
using Engine.Editor.Attributes;
using Microsoft.Practices.Unity;
using Microsoft.Practices.ServiceLocation;
using Engine.Editor.Services;
using Engine.Editor.Modules;
using Prism.Modularity;

namespace Engine.Editor
{
    class BootStrapper : UnityBootstrapper
    {
        [Interceptor()]
        protected override DependencyObject CreateShell()
        {
            return ServiceLocator.Current.GetInstance<MainView>();
        }

        [Interceptor()]
        protected override void InitializeModules()
        {
            base.InitializeModules();

            Application.Current.MainWindow = (MainView)Shell;
            Application.Current.MainWindow.Show();
        }

        [Interceptor()]
        protected override void ConfigureContainer()
        {
            base.ConfigureContainer();

            Container.RegisterInstance<IDispatcherService>(new DispatcherService(Application.Current.Dispatcher), new ContainerControlledLifetimeManager());
        }

        //[Interceptor()]
        //protected override IRegionBehaviorFactory ConfigureDefaultRegionBehaviors()
        //{
        //    var regionBehaviorFactory = ServiceLocator.Current.GetInstance<IRegionBehaviorFactory>();

        //    if (ReferenceEquals(regionBehaviorFactory, null) == false)
        //    {
        //    }

        //    return regionBehaviorFactory;
        //}

        [Interceptor()]
        protected override void ConfigureModuleCatalog()
        {
            base.ConfigureModuleCatalog();

            var moduleCatalog = (ModuleCatalog)ModuleCatalog;

            moduleCatalog.AddModule(typeof(GeneralEditorModule));
        }
    }
}
