using PropertyChanged;
using System;
using System.Collections.ObjectModel;

namespace Engine.Editor.ViewModels.ToolBar
{
    [ImplementPropertyChanged()]
    class ToolBarViewModel : AbstractViewModel<ObservableCollection<ToolBarControlViewModel>>
    {
        /// <summary>
        /// 
        /// </summary>
        public ToolBarViewModel()
        {
            Model = new ObservableCollection<ToolBarControlViewModel>();
        }

        /// <summary>
        /// 
        /// </summary>
        public Type ElementTemplateSelector { get; set; }
    }
}
