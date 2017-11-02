using System;
using System.Linq;
using System.Runtime.InteropServices;
using GDI = System.Drawing;
using System.Windows.Media;
using System.IO;
using System.Windows;
using System.Windows.Media.Imaging;
using System.Windows.Interop;
using PropertyChanged;
using System.Collections.ObjectModel;
using Microsoft.Practices.Unity;
using Engine.Editor.Services;

namespace Engine.Editor.ViewModels.FileSystemTreeView
{
    [ImplementPropertyChanged()]
    class FileSystemElementViewModel : AbstractViewModel
    {
        [Dependency()]
        private IDispatcherService DispatcherService { get; set; }

        public bool AllowItemStateChange { get; set; }

        [DoNotNotify()]
        public FileSystemElementViewModel Parent { get; }

        [DoNotNotify()]
        public ObservableCollection<FileSystemElementViewModel> Children { get; } = new ObservableCollection<FileSystemElementViewModel>();

        [DoNotNotify()]
        public FileSystemInfo FileSystemInformation { get; }

        public ImageSource ImageSource { get; private set; }
        public bool IsSelected { get; set; }

        [DoNotNotify()]
        private DriveInfo Drive { get; }

        public FileSystemElementViewModel(FileSystemElementViewModel parent, FileSystemInfo fileSystemInfo)
        {
            if (this is DummyFileSystemElementViewModel) return;

            Parent = parent;
            FileSystemInformation = fileSystemInfo;

            if (fileSystemInfo is DirectoryInfo)
            {
                AllowItemStateChange = true;
                ImageSource = Win32.GetImageSource(fileSystemInfo.FullName, new Size(16, 16), Win32.ItemState.Close);
                AddDummy();
            }
            else if (fileSystemInfo is FileInfo)
            {
                AllowItemStateChange = false;
                ImageSource = Win32.GetImageSource(fileSystemInfo.FullName, new Size(16, 16));
            }
        }

        public FileSystemElementViewModel(DriveInfo driveInformation)
            : this(null, driveInformation.RootDirectory)
        {
            Drive = driveInformation;
        }

        private bool _isExpanded;
        public bool IsExpanded
        {
            get { return _isExpanded; }
            set
            {
                if (value)
                {
                    if (AllowItemStateChange)
                        ImageSource = Win32.GetImageSource(FileSystemInformation.FullName, new Size(16, 16), Win32.ItemState.Open);

                    if (HasDummy())
                    {
                        RemoveDummy();
                        ExploreDirectoriesAsync();
                        ExploreFilesAsync();
                    }
                }
                else
                {
                    if (AllowItemStateChange)
                        ImageSource = Win32.GetImageSource(FileSystemInformation.FullName, new Size(16, 16), Win32.ItemState.Close);
                }

                _isExpanded = value;

                OnPropertyChanged();
            }
        }

        protected void ExploreDirectoriesAsync()
        {
            if ((Drive == null || Drive.IsReady) == false) return;

            if (FileSystemInformation is DirectoryInfo)
            {
                var directoryInfoElements = ((DirectoryInfo)FileSystemInformation).EnumerateDirectories()
                .Where(element => element.Attributes.HasFlag(FileAttributes.Directory))
                .Where(element => element.Attributes.HasFlag(FileAttributes.System | FileAttributes.Hidden) == false);

                foreach (var element in directoryInfoElements.OrderBy(element => element.Name).Select(element => new FileSystemElementViewModel(this, element)))
                    Children.Add(element);
            }
        }

        protected void ExploreFilesAsync()
        {
            if ((Drive == null || Drive.IsReady) == false) return;

            if (FileSystemInformation is DirectoryInfo)
            {
                var fileInfoElements = ((DirectoryInfo)FileSystemInformation).EnumerateFiles()
                    .Where(element => element.Attributes.HasFlag(FileAttributes.System | FileAttributes.Hidden) == false);

                foreach (var element in fileInfoElements.OrderBy(element => element.Name).Select(element => new FileSystemElementViewModel(this, element)))
                    Children.Add(element);
            }
        }

        #region DummyMethods

        protected void AddDummy()
        {
            Children.Add(new DummyFileSystemElementViewModel());
        }

        protected DummyFileSystemElementViewModel GetDummy() => Children.OfType<DummyFileSystemElementViewModel>().FirstOrDefault();

        protected bool HasDummy() => ReferenceEquals(GetDummy(), null) == false;

        protected void RemoveDummy()
        {
            Children.Remove(GetDummy());
        }

        #endregion DummyMethods

        private static class Win32
        {
            public const uint FileAttributeDirectory = 0x00000010;
            public const uint FileAttributeFile = 0x00000100;
            public const uint FileInfoIcon = 0x000000100;
            public const uint FileInfoLargeicon = 0x000000000;
            public const uint FileInfoOpenicon = 0x000000002;
            public const uint FileInfoSmallicon = 0x000000001;
            public const uint FileInfoUsefileattributes = 0x000000010;

            [DllImport("user32.dll", SetLastError = true)]
            [return: MarshalAs(UnmanagedType.Bool)]
            public static extern bool DestroyIcon(IntPtr hIcon);

            [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto)]
            public struct Win32FileInfo
            {
                public IntPtr hIcon;
                public int iIcon;
                public uint dwAttributes;

                [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 260)]
                public string szDisplayName;

                [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 80)]
                public string szTypeName;
            };

            public enum IconSize : short { Small, Large }

            public enum ItemState : short { Undefined, Open, Close }

            public enum ItemType : short { Directory, File }

            public static GDI.Icon GetIcon(string path, ItemType type, IconSize size, ItemState state)
            {
                var flags = FileInfoIcon | FileInfoUsefileattributes;
                var attribute = Equals(type, ItemType.Directory) ? FileAttributeDirectory : FileAttributeFile;

                if (Equals(type, ItemType.Directory) && Equals(state, ItemState.Open))
                    flags += FileInfoOpenicon;

                flags += Equals(size, IconSize.Small) ? FileInfoSmallicon : FileInfoLargeicon;

                var fileInfo = new Win32FileInfo();
                if (Equals(GetFileInfo(path, attribute, out fileInfo, (uint)Marshal.SizeOf(fileInfo), flags), IntPtr.Zero)) throw Marshal.GetExceptionForHR(Marshal.GetHRForLastWin32Error());
                try
                {
                    GDI.Icon.FromHandle(fileInfo.hIcon);
                    return (GDI.Icon)GDI.Icon.FromHandle(fileInfo.hIcon).Clone();
                }
                finally
                {
                    DestroyIcon(fileInfo.hIcon);
                }
            }

            [DllImport("shell32.dll", CharSet = CharSet.Auto, EntryPoint = "SHGetFileInfo")]
            public static extern IntPtr GetFileInfo(string pszPath, uint dwFileAttributes, out Win32FileInfo psfi, uint cbFileInfo, uint uFlags);

            public static ImageSource GetImageSource(string filename, Size size)
            {
                using (var icon = GetIcon(Path.GetExtension(filename), ItemType.File, IconSize.Small, ItemState.Undefined))
                {
                    return Imaging.CreateBitmapSourceFromHIcon(icon.Handle, Int32Rect.Empty, BitmapSizeOptions.FromWidthAndHeight((int)size.Width, (int)size.Height));
                }
            }

            public static ImageSource GetImageSource(string directory, Size size, ItemState folderType)
            {
                using (var icon = GetIcon(directory, ItemType.Directory, IconSize.Large, folderType))
                {
                    return Imaging.CreateBitmapSourceFromHIcon(icon.Handle, Int32Rect.Empty, BitmapSizeOptions.FromWidthAndHeight((int)size.Width, (int)size.Height));
                }
            }
        }
    }
}
