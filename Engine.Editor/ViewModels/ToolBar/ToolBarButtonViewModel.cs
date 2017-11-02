using PropertyChanged;
using System.Windows.Input;

namespace Engine.Editor.ViewModels.ToolBar
{
    [ImplementPropertyChanged()]
    class ToolBarButtonViewModel : ToolBarControlViewModel
    {
        public string Content { get; set; }

        public ICommand Command { get; set; }
    }
}
