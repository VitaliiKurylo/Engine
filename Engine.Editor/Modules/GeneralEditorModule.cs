using Engine.Editor.Views;
using Microsoft.Practices.ServiceLocation;
using Prism.Modularity;
using Prism.Regions;

namespace Engine.Editor.Modules
{
    class GeneralEditorModule : IModule
    {
        private readonly IRegionViewRegistry _regionViewRegistry;
        private readonly IServiceLocator _serviceLocator;

        public GeneralEditorModule(IRegionViewRegistry regionViewRegistry, IServiceLocator serviceLocator)
        {
            _regionViewRegistry = regionViewRegistry;
            _serviceLocator = serviceLocator;
        }

        public void Initialize()
        {
            _regionViewRegistry.RegisterViewWithRegion("TreeViewRegion", () => _serviceLocator.GetInstance<FileSystemTreeView>());
        }
    }
}
