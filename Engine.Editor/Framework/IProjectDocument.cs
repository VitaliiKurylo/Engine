using Engine.Editor.Models;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Windows.Input;

namespace Engine.Editor.Framework
{
    interface IProjectDocument
    {
        List<AbstractSelectableModel> SelectedElements { get; }

        ObservableCollection<AbstractModel> Elements { get; }

        ICommand ClearSelection { get; }

        ICommand AddElement { get; }

        ICommand RemoveElement { get; }
    }
}
