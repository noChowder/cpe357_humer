4. The pointers all have different addresses. Their values of cp and pp match the address they point to. The value of ap points to the first char of the array.

5. A pointer can point to itself by type casting. The pointer will point to its own position, but it needs to be type casted in some way because its value type would be different than its address.

6. In the for loop, p != '\0', the p needs to be *p because p represents the address and not the value at that position.

7. Pointers and arrays are similar because they store data/values and can be referenced. The difference is that arrays store data of the same type, while pointers contain addresses of values.