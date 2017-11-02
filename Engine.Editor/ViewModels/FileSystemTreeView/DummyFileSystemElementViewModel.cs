using System.IO;

namespace Engine.Editor.ViewModels.FileSystemTreeView
{
    class DummyFileSystemElementViewModel : FileSystemElementViewModel
    {
        /// <summary>
        ///     Initializes a new instance of the <see cref="DummyFileSystemElementViewModel" /> class.
        /// </summary>
        public DummyFileSystemElementViewModel()
            : base(new DriveInfo(@"C:\"))
        {
        }
    }
}
