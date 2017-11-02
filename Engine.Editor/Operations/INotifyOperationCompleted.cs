using System.ComponentModel;
using System.Runtime.CompilerServices;

namespace Engine.Editor.Operations
{
    interface INotifyOperationCompleted : INotifyPropertyChanged
    {
        void RaisePropertyChanged([CallerMemberName] string propertyName = null);
    }
}
