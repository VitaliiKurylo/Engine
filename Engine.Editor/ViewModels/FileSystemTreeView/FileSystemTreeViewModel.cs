using PropertyChanged;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;

namespace Engine.Editor.ViewModels.FileSystemTreeView
{
    [ImplementPropertyChanged()]
    class FileSystemTreeViewModel : AbstractViewModel<ObservableCollection<FileSystemElementViewModel>>
    {
        public FileSystemTreeViewModel()
        {
            Model = new ObservableCollection<FileSystemElementViewModel>(DriveInfo.GetDrives()
                .OrderBy(element => element.Name)
                .Select(element => new FileSystemElementViewModel(element)));
        }
    }
}
