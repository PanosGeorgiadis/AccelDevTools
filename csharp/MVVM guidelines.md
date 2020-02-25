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


### Step 1

The open source project mvvmlight (https://github.com/lbugnion/mvvmlight) contains sources with common code for all WPF applications. Select the files necessary to use the MVVM framework in your application.
(note: this step is optional, as all behavior of the pattern can be reproduced - but it will save some time)
