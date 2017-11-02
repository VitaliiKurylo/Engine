using Engine.Editor.ViewModels;
using System.Windows.Controls;

namespace Engine.Editor.Views
{
    abstract class AbstractView<TViewModel> : UserControl
        where TViewModel : AbstractViewModel
    {
        /// <summary>
        /// Gets or sets the view model.
        /// </summary>
        /// <value>
        /// The view model.
        /// </value>
        public TViewModel ViewModel
        {
            get { return DataContext as TViewModel; }
            set { DataContext = value; }
        }
    }
}
