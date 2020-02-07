// If you're using .NET 4 (or higher), you can use the System.Lazy<T> type to make the laziness really simple.
// All you need to do is pass a delegate to the constructor which calls the Singleton constructor - which
// is done most easily with a lambda expression.
// It's simple and performs well. It also allows you to check whether or not the instance has been created
// yet with the IsValueCreated property, if you need that.
// The code above implicitly uses LazyThreadSafetyMode.ExecutionAndPublication as the thread safety mode
// for the Lazy<Singleton>. Depending on your requirements, you may wish to experiment with other modes. 
public sealed class Singleton
{
    private static readonly Lazy<Singleton> lazy =
        new Lazy<Singleton>
            (() => new Singleton());

    public static Singleton Instance { get { return lazy.Value; } }

    private Singleton()
    {
    }
}
