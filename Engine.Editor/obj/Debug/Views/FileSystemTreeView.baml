   M S B A M L   `   `   `  ����H  DEngine.Editor, Version=1.0.0.0, Culture=neutral, PublicKeyToken=nulli9clr-namespace:Engine.Editor.ViewModels.FileSystemTreeView+Engine.Editor.ViewModels.FileSystemTreeView  5      9!clr-namespace:Engine.Editor.ViewsEngine.Editor.Views  5      ,   &Engine.Editor.Views.FileSystemTreeView   5      R NSystem.Xaml, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089\ XPresentationFramework, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35R NWindowsBase, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35W SPresentationCore, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35:x,http://schemas.microsoft.com/winfx/2006/xaml     5      D 9http://schemas.microsoft.com/winfx/2006/xaml/presentation    5      N
viewModels9clr-namespace:Engine.Editor.ViewModels.FileSystemTreeView    5      1views!clr-namespace:Engine.Editor.Views    5      .��5      R�   %� IsVirtualizing  .  %� VirtualizationMode$ 	Recycling=�5	      ��5
      ��  �� Path$ Model� � 	Resources 5   
    �� DataTypeL  FEngine.Editor.ViewModels.FileSystemTreeView.FileSystemElementViewModel �� ItemsSource �� Orientation��%�  (��       5   '   *, +)�� 6   "  6'    6a   �� $ Children�.R�5      �� $	��0,2q�6   $ 
Horizontal=�6*   .+�5      �� $��0,1,0,1q�6   $��Center=�5   $   $
��None=�5   $   ��5   $   �� $ ImageSource�5   $   �� 5      $��5,0,5,0q�6    $��Center=�5   (   ��5   (   �� $  FileSystemInformation.Name�5      5      5      5      ��5   
   �� 5       �� 
TargetType" Q�6   .�5      ��  �� Property �@Q�
IsExpanded	 �� Value	 6/   �� $ 
IsExpanded�
 �� Mode$
 TwoWay=��� 5       �@Q�
IsSelected	 6/   �� $ 
IsSelected�$
 TwoWay=��� 5      -  'System.Windows.Input.KeyboardNavigation �@ AcceptsReturn	 . 6E   5      5       5!      