using System.ComponentModel;
using System.Runtime.CompilerServices;

namespace Engine.Editor.ViewModels
{
    abstract class AbstractViewModel : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        protected AbstractViewModel() { }

        public void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            if (ReferenceEquals(PropertyChanged, null) == false)
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
        }
    }

    abstract class AbstractViewModel<TModel> : AbstractViewModel where TModel : class
    {
        protected AbstractViewModel() { }

        public virtual TModel Model { get; set; }
    }
}
