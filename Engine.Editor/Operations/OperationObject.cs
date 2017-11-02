using System.ComponentModel;
using System.Runtime.CompilerServices;

namespace Engine.Editor.Operations
{
    abstract class OperationObject : INotifyOperationCompleted
    {
        protected OperationObject()
        {
        }

        public event PropertyChangedEventHandler PropertyChanged;
        
        public void RaisePropertyChanged([CallerMemberName] string propertyName = null)
        {
            if (ReferenceEquals(PropertyChanged, null) == false)
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
