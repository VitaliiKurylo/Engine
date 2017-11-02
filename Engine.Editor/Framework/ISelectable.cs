using System.Collections.Generic;
using System.Windows.Input;

namespace Engine.Editor.Framework
{
    interface ISelectable
    {
        ICommand SelectCommand { get; }
        
        bool IsSelected { get; set; }
        
        List<ISelectable> SelectedElements { get; }
    }
}
