using Engine.Editor.Binding;
using Engine.Editor.ViewModels;

namespace Engine.Editor.Views
{
    /// <summary>
    /// Interaction logic for MainView.xaml
    /// </summary>
    partial class MainView
    {
        public MainView()
        {
            InitializeComponent();
            
            ViewModel = new MainViewModel();
            ViewModel.ToolBars.Add(new ToolBarBinding(ViewModel).GetBinding());
        }

        /// <summary>
        /// 
        /// </summary>
        internal MainViewModel ViewModel
        {
            get { return DataContext as MainViewModel; }
            set { DataContext = value; }
        }
    }
}
