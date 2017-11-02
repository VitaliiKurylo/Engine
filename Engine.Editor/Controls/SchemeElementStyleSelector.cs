using System.Windows;
using System.Windows.Controls;

namespace Engine.Editor.Controls
{
    class SchemeElementStyleSelector : StyleSelector
    {
        /// <summary>
        /// 
        /// </summary>
        public Style ConnectorStyle { get; set; }

        /// <summary>
        /// 
        /// </summary>
        public Style SchemeElementStyle { get; set; }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="item"></param>
        /// <param name="container"></param>
        /// <returns></returns>
        public override Style SelectStyle(object item, DependencyObject container)
        {
            return base.SelectStyle(item, container);
        }
    }
}
