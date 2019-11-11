## Moving Forward

- Remove `Stream` class and replace `Agent::Add` with `Agent::Extend` that
    returns a new `Agent` reference with a set of extended labels.
- Add locks around queue operations to ensure thread safety.
- Add worker to automatically flush queue every so often.
- Implement "Log Level Filtering".
