using Engine.Editor.ViewModels.ToolBar;
using System.Windows;
using System.Windows.Controls;

namespace Engine.Editor.Controls
{
    class ToolBarElementTemplateSelector : DataTemplateSelector
    {
        /// <summary>
        /// 
        /// </summary>
        public DataTemplate ButtonTemplate { get; set; }

        /// <summary>
        /// 
        /// </summary>
        public DataTemplate SeparatorTemplate { get; set; }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="item"></param>
        /// <param name="container"></param>
        /// <returns></returns>
        public override DataTemplate SelectTemplate(object item, DependencyObject container)
        {
            if (item is ToolBarButtonViewModel) return ButtonTemplate;

            if (item is ToolBarSeparatorViewModel) return SeparatorTemplate;

            return base.SelectTemplate(item, container);
        }
    }
}
