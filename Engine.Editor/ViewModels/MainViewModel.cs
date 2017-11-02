using Engine.Editor.Attributes;
using Engine.Editor.Commands;
using Engine.Editor.Controls;
using Engine.Editor.ViewModels.ToolBar;
using PropertyChanged;
using System.Collections.ObjectModel;
using System.Threading.Tasks;
using System.Windows.Input;

namespace Engine.Editor.ViewModels
{
    [ToolBar(typeof(ToolBarElementTemplateSelector))]
    [ImplementPropertyChanged()]
    class MainViewModel : AbstractViewModel
    {
        private ICommand _fileNewCommand;
        private ICommand _fileOpenCommand;

        [ToolBarButton("New")]
        public ICommand FileNewCommand { get { return _fileNewCommand ?? (_fileNewCommand = new RelayCommandAsync(FileNewCommandExecute)); } }

        [ToolBarButton("Open")]
        public ICommand FileOpenCommand { get { return _fileOpenCommand ?? (_fileOpenCommand = new RelayCommand(FileOpenCommandExecute)); } }
        
        private async Task FileNewCommandExecute(object parameter) => await Task.Delay(2000);
        
        private void FileOpenCommandExecute(object parameter)
        {
            var openDialog = new Microsoft.Win32.OpenFileDialog();
            var result = openDialog.ShowDialog();
        }
        
        public MainViewModel()
        {
        }
        
        public ObservableCollection<ToolBarViewModel> ToolBars { get; } = new ObservableCollection<ToolBarViewModel>();
    }
}
