using Engine.Editor.Models;
using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using static System.Math;

namespace Engine.Editor.Controls
{
    abstract class AbstractCanvas : Canvas
    {
        protected AbstractCanvas()
        {
            AllowDrop = true;
        }

        protected virtual void HitTesting(Point hitPoint)
        {
            var @object = InputHitTest(hitPoint) as DependencyObject;
            while (ReferenceEquals(@object, null) == false && @object.GetType().Equals(GetType()) == false)
            {
                @object = VisualTreeHelper.GetParent(@object);
            }
        }

        protected override Size MeasureOverride(Size constraint)
        {
            var constraintSize = default(Size);
            foreach (FrameworkElement element in InternalChildren)
            {
                var elementLeft = GetLeft(element);
                var elementTop = GetTop(element);
                elementLeft = double.IsNaN(elementLeft) ? 0 : elementLeft;
                elementTop = double.IsNaN(elementTop) ? 0 : elementTop;

                element.Measure(constraint);

                var canvasElement = (AbstractModel)element.DataContext;
                canvasElement.Size = element.DesiredSize;

                var elementDesiredSize = element.DesiredSize;
                if ((double.IsNaN(elementDesiredSize.Width) || double.IsNaN(elementDesiredSize.Height)) == false)
                {
                    constraintSize.Width = Max(constraintSize.Width, elementLeft + elementDesiredSize.Width);
                    constraintSize.Height = Max(constraintSize.Height, elementTop + elementDesiredSize.Height);
                }
            }

            constraintSize.Width += 10;
            constraintSize.Height += 10;

            return constraintSize;
        }
    }
}
