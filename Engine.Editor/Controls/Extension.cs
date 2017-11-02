using Engine.Editor.ViewModels.ToolBar;
using System.Collections;
using System.Linq;
using System.Windows;
using System.Windows.Controls;

namespace Engine.Editor.Controls
{
    class Extension
    {
        #region ToolBarTray
        
        public static readonly DependencyProperty ItemsSourceProperty = DependencyProperty.RegisterAttached("ItemsSource", typeof(IEnumerable), typeof(Extension),
                new UIPropertyMetadata(null, OnItemsSourcePropertyChanged));
        
        public static void OnItemsSourcePropertyChanged(DependencyObject @object, DependencyPropertyChangedEventArgs eventArguments)
        {
            var targetElement = @object as ToolBarTray;
            if (targetElement == null) return;

            targetElement.ToolBars.Clear();

            if (eventArguments.NewValue == null) return;

            foreach (var item in ((IEnumerable)eventArguments.NewValue).OfType<ToolBarViewModel>())
            {
                targetElement.ToolBars.Add(new ToolBar
                {
                    ItemTemplateSelector = targetElement.FindResource(item.ElementTemplateSelector) as ToolBarElementTemplateSelector,
                    ItemsSource = item.Model
                });
            }
        }
        
        public static IEnumerable GetItemsSource(DependencyObject @object) => (IEnumerable)@object.GetValue(ItemsSourceProperty);
        
        public static void SetItemsSource(DependencyObject @object, IEnumerable value)
        {
            @object.SetValue(ItemsSourceProperty, value);
        }

        #endregion ToolBarTray
    }
}
