comments in the sources marked with @note.

Your code is sometimes overcomplicated. You could have made things more compact with the use of if constexpr instead of separating methods for the row major and column major cases. Also becouse you have now a lot of code duplicated, and code duplication makes mainenace more difficult and readeability worse.

Yet, in general the code looks good.
