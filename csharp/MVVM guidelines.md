# MVVM guidelines

These guidelines apply to .NET Framework for Windows applications written in C# with the WPF subsystem.

> [WPF](https://github.com/dotnet/wpf) is a framework similar to (and an evolution of) WinForms, which separates user interface from business logic


## Implementation

### Project Structure

It is preferred that a WPF project is using separate folders for:
- ViewModels
- Models
- Views
- Utilities (common functionality)

A set of template classes is provided to make data binding with the ViewModel easier:
* `RelayCommand.cs`
* `ObservableObject.cs`

