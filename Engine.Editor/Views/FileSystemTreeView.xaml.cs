using Engine.Editor.ViewModels.FileSystemTreeView;

namespace Engine.Editor.Views
{
    partial class FileSystemTreeView
    {
        public FileSystemTreeView()
        {
            InitializeComponent();

            ViewModel = new FileSystemTreeViewModel();
        }
    }
}
